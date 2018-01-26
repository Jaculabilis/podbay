#include <argp.h>

/* Barcodes are just strings. */
typedef char* barcode;

/*
 * "Linked List String", a linked list of characters.
 * The head of the list is the last character in the string.
 */
typedef struct ll_string {
  char character;
  struct ll_string *next;
} ll_string;

/*
 * Function prototypes
 */
error_t
parse_opt(int key, char *arg, struct argp_state *state);

static barcode
await_next_barcode(int fd);

static char
decode_next_event(int fd);

static void *
Calloc(size_t size);

static void *
Malloc(size_t size);

static char *
ll_string_to_c_string(ll_string *l);

static ll_string *
ll_string_append(ll_string *l, char c);

static unsigned int
ll_string_len(ll_string *l);

static void
ll_string_free(ll_string *l);

/*
 * Arguments used by the read_scan application
 */
typedef struct args {
  char *zmq_endpoint;
  char *dev_id;
} args;

/*
 * Descriptions of command line arguments we'd like to parse.
 */
static const struct argp_option options[] = {
        {
          .name  = "zmq_endpoint",
          .key   = 'e',
          .arg   = "zmq_endpoint",
          .flags = 0,
          .doc   = "ZeroMQ endpoint to publish barcodes",
          .group = 0
        },
        {
          .name = "dev_id",
          .key  = 'd',
          .arg  = "dev_id",
          .flags = 0,
          .doc = "Device id of the barcode scanner (in /dev/input/by-id)",
          .group = 0
        },
        // Need one zero-struct to terminate, per the argp spec.
        { }
};

static const struct argp argp = {
  .options  = options,
  .parser   = parse_opt,
  .args_doc = NULL,
  .doc      = "Acquire barcode data and publish to a ZeroMQ channel\v",
  .children = NULL
};
