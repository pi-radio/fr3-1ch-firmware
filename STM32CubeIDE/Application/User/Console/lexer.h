
typedef enum {
  TOK_NONE,
  TOK_EOL,
  TOK_ID,
  TOK_INT,
  TOK_DOUBLE,
  TOK_STR,

  TOK_SPIW,
  TOK_ERASE,
  TOK_CONFIG,
  TOK_READ,
  TOK_REDRAW,
  TOK_LMX,
  TOK_PROG,
  TOK_WRITE,
  TOK_REG,
  TOK_BOOTLOADER,

  TOK_ERROR
} token_type_t;

typedef struct {
  token_type_t token_type;
  union {
    struct {
      char str[128];
      int len;
    } s;
    int i;
    double d;
  };
} token_t;

extern void get_token(token_t *);
extern void lexer_set_line(const char *s, int len);
