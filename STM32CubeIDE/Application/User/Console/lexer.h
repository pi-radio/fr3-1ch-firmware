
typedef enum {
  TOK_ERROR = -1,
  TOK_EOL = 0,
  TOK_ID = 1,
  TOK_INT = 2,
  TOK_DOUBLE = 3,
  TOK_STR = 4
} token_type_t;

typedef struct {
  token_type_t token_type;
  union {
    char str[128];
    int i;
    double d;
  };
} token_t;

extern void get_token(token_t *);
