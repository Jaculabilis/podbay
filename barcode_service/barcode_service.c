#include <argp.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <czmq.h>

#include "barcode_service.h"

#define DEV_ID_PREFIX "/dev/input/by-id/"

int
main(int argc, char **argv)
{
  zsock_t *zsock;
  args cli_args = {};
  barcode barcode;
  int input_fd;

  /* Read command line arguments. */
  if (argp_parse(&argp, argc, argv, 0, NULL, &cli_args) != 0)
  {
    fprintf(stderr, "Could not read command line arguments!\n");
    exit(1);
  }

  /* Acquire input device event fd. */
  const int path_size = strlen(DEV_ID_PREFIX) + strlen(cli_args.dev_id);
  char *path = Calloc(path_size + 1);
  strcpy(path, DEV_ID_PREFIX);
  strcat(path, cli_args.dev_id);
  if ((input_fd = open(path, O_RDONLY)) < 0)
  {
    fprintf(stderr, "Could not open input device at %s!\n", path);
    exit(input_fd);
  }
  free(path);

  /* Open ZMQ 'PUB' socket. */
  zsock = zsock_new_pub((const char *) cli_args.zmq_endpoint);
  if (zsock == NULL)
  {
    fprintf(stderr, "Failed to bind new ZMQ socket!");
  }

  /* Acquire and send barcodes through the ZMQ socket. */
  printf("Ready, awaiting barcodes.\n");
  while (true)
  {
    barcode = await_next_barcode(input_fd);
    if (barcode[0] == '\x00')
    {
      /* Newline is pressed twice. Ignore any 'empty' barcodes. */
      continue;
    }
    if (zsock_send(zsock, "s", barcode) < 0)
    {
      fprintf(stderr, "Sending barcode failed!\n");
    }
    printf("Published barcode %s!\n", barcode);
    free(barcode);
  }
}

/**
 * Read from an input device file descriptor given by fd. Return the barcode
 * that was read. The barcode will need to be returned to the heap.
 */
static barcode
await_next_barcode(int fd)
{
  char c;
  barcode barcode;
  ll_string *ll_barcode = NULL;
  while ((c = decode_next_event(fd)) != '\n')
  {
    ll_barcode = ll_string_append(ll_barcode, c);
  }
  barcode = ll_string_to_c_string(ll_barcode);
  ll_string_free(ll_barcode);
  return barcode;
}

/**
 * Decode an input event from the file descriptor.
 * If successful, return a char cast to an int containing an interpretation
 * of the character read, in lower case.
 * Not all input events are as straightforward as 'key x was depressed'. To this
 * end, this function will block and wait for the next input event until it sees
 * a key being pressed.
 */
static char
decode_next_event(int fd)
{
  struct input_event event;
  while (true)
  {
    /*
     * Read the next event.
     */
    ssize_t n_read = read(fd, &event, sizeof(struct input_event));
    if (n_read != sizeof(struct input_event)) {
      fprintf(stderr, "Read interrupted!\n");
      continue;
    }

    /*
     * If we received a keypress event, decode the keycode responsible.
     * (Event value 1 = key depressed)
     */
    if (event.type == EV_KEY && event.value == 1) {
      switch (event.code) {
#define DECODE_KEY(key_name, decode_char) case (KEY_##key_name): return decode_char
        DECODE_KEY(ENTER, '\n');
        DECODE_KEY(0, '0');
        DECODE_KEY(1, '1');
        DECODE_KEY(2, '2');
        DECODE_KEY(3, '3');
        DECODE_KEY(4, '4');
        DECODE_KEY(5, '5');
        DECODE_KEY(6, '6');
        DECODE_KEY(7, '7');
        DECODE_KEY(8, '8');
        DECODE_KEY(9, '9');
        DECODE_KEY(A, 'a');
        DECODE_KEY(B, 'b');
        DECODE_KEY(C, 'c');
        DECODE_KEY(D, 'd');
        DECODE_KEY(E, 'e');
        DECODE_KEY(F, 'f');
        DECODE_KEY(G, 'g');
        DECODE_KEY(H, 'h');
        DECODE_KEY(I, 'i');
        DECODE_KEY(J, 'j');
        DECODE_KEY(K, 'k');
        DECODE_KEY(L, 'l');
        DECODE_KEY(M, 'm');
        DECODE_KEY(N, 'n');
        DECODE_KEY(O, 'o');
        DECODE_KEY(P, 'p');
        DECODE_KEY(Q, 'q');
        DECODE_KEY(R, 'r');
        DECODE_KEY(S, 's');
        DECODE_KEY(T, 't');
        DECODE_KEY(U, 'u');
        DECODE_KEY(V, 'v');
        DECODE_KEY(W, 'w');
        DECODE_KEY(X, 'x');
        DECODE_KEY(Y, 'y');
        DECODE_KEY(Z, 'z');
      default:
        fprintf(stderr, "Unknown key code %d (?!)", event.code);
        continue;
      }
    }
  }
}

/**
 * Parsing hook for each argument encountered. This populates an 'args' struct
 * and also verifies both that all necessary options are specified and that no
 * unknown arguments are offered.
 */
error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
  args* cli_args = state->input;

  // Verify that both arguments have values.
  switch (key) {
  // Device ID
  case 'd':
    cli_args->dev_id = arg;
    break;

  // ZMQ Endpoint
  case 'e':
    cli_args->zmq_endpoint = arg;
    break;

  // Don't support positional arguments.
  case ARGP_KEY_ARG:
    argp_usage(state);
    break;

  // Verify arguments passed at the end.
  case ARGP_KEY_END:
    if (cli_args->dev_id == NULL || cli_args->zmq_endpoint == NULL)
      argp_usage(state);
    break;

  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/**
 * Attempt to malloc `size` bytes. On failure, causes the program to exit.
 */
static void *
Malloc(size_t size)
{
  void *ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "Could not malloc %zu bytes!\n", size);
    exit(1);
  }
  return ptr;
}

/**
 * Attempt to malloc `size` bytes. On failure, causes the program to exit.
 */
static void *
Calloc(size_t size)
{
  void *ptr = calloc(1, size);
  if (ptr == NULL) {
    fprintf(stderr, "Could not malloc %zu bytes!\n", size);
    exit(1);
  }
  return ptr;
}

/**
 * VARIOUS ll_string FUNCTIONS
 */

static char *
ll_string_to_c_string(ll_string *l)
{
  ll_string *rest = l;
  unsigned int size = ll_string_len(l);
  char *c_string = Malloc(size + 1);

  // NUL-terminate the C string.
  c_string[size] = '\x00';

  // Copy the rest of the linked list into the C-string.
  for (int i = size; i > 0; i--)
  {
    c_string[i] = rest->character;
    rest = rest->next;
  }

  return c_string;
}

static ll_string *
ll_string_append(ll_string *l, char c)
{
  ll_string *new_head = Malloc(sizeof(ll_string));
  new_head->character = c;
  new_head->next = l;
  return new_head;
}

static unsigned int
ll_string_len(ll_string *l)
{
  if (l == NULL) {
    return 0;
  } else {
    return 1 + ll_string_len(l->next);
  }
}

static void
ll_string_free(ll_string *l)
{
  if (l != NULL) {
    ll_string *next = l->next;
    free(l);
    ll_string_free(next);
  }
}
