#include "ruby.h"
#include "ruby/encoding.h"

static VALUE rb_cERB, rb_mEscape, rb_cCGI;
static ID id_escapeHTML;

#define HTML_ESCAPE_MAX_LEN 6

static const struct {
    uint8_t len;
    char str[HTML_ESCAPE_MAX_LEN+1];
} html_escape_table[UCHAR_MAX+1] = {
#define HTML_ESCAPE(c, str) [c] = {rb_strlen_lit(str), str}
    HTML_ESCAPE('\'', "&#39;"),
    HTML_ESCAPE('&', "&amp;"),
    HTML_ESCAPE('"', "&quot;"),
    HTML_ESCAPE('<', "&lt;"),
    HTML_ESCAPE('>', "&gt;"),
#undef HTML_ESCAPE
};

static inline void
preserve_original_state(VALUE orig, VALUE dest)
{
    rb_enc_associate(dest, rb_enc_get(orig));
}

static inline long
escaped_length(const long len)
{
    if (len >= LONG_MAX / HTML_ESCAPE_MAX_LEN) {
        ruby_malloc_size_overflow(len, HTML_ESCAPE_MAX_LEN);
    }
    return len * HTML_ESCAPE_MAX_LEN;
}

static inline VALUE
optimized_escape_html(VALUE str, bool return_self)
{
    const char *cstr;
    long str_len;
    RSTRING_GETMEM(str, cstr, str_len);
    const char *end = cstr + str_len;

    while (cstr < end) {
        const unsigned char c = *cstr;
        if (html_escape_table[c].len) {
            break;
        }
        cstr++;
    }

    if (cstr == end) {
        // Nothing to escape.
        if (return_self) {
            return str;
        }
        return rb_str_dup(str);
    }

    VALUE vbuf;
    const long remaining = end - cstr;
    const long scanned = str_len - remaining;
    char *buf = ALLOCV_N(char, vbuf, scanned + escaped_length(remaining));
    memcpy(buf, RSTRING_PTR(str), scanned);

    char *dest = buf + scanned;
    while (cstr < end) {
        const unsigned char c = *cstr++;
        uint8_t len = html_escape_table[c].len;
        if (len) {
            memcpy(dest, html_escape_table[c].str, len);
            dest += len;
        }
        else {
            *dest++ = c;
        }
    }

    VALUE escaped = str;
    if (RSTRING_LEN(str) < (dest - buf)) {
        escaped = rb_str_new(buf, dest - buf);
        preserve_original_state(str, escaped);
    }
    ALLOCV_END(vbuf);
    return escaped;
}

static inline long
old_escaped_length(VALUE str)
{
    const long len = RSTRING_LEN(str);
    if (len >= LONG_MAX / HTML_ESCAPE_MAX_LEN) {
        ruby_malloc_size_overflow(len, HTML_ESCAPE_MAX_LEN);
    }
    return len * HTML_ESCAPE_MAX_LEN;
}

static VALUE
old_optimized_escape_html(VALUE str)
{
    VALUE vbuf;
    char *buf = ALLOCV_N(char, vbuf, old_escaped_length(str));
    const char *cstr = RSTRING_PTR(str);
    const char *end = cstr + RSTRING_LEN(str);

    char *dest = buf;
    while (cstr < end) {
        const unsigned char c = *cstr++;
        uint8_t len = html_escape_table[c].len;
        if (len) {
            memcpy(dest, html_escape_table[c].str, len);
            dest += len;
        }
        else {
            *dest++ = c;
        }
    }

    VALUE escaped = str;
    if (RSTRING_LEN(str) < (dest - buf)) {
        escaped = rb_str_new(buf, dest - buf);
        preserve_original_state(str, escaped);
    }
    ALLOCV_END(vbuf);
    return escaped;
}

/*
 * ERB::Util.html_escape is similar to CGI.escapeHTML but different in the following two parts:
 *
 * * ERB::Util.html_escape converts an argument with #to_s first (only if it's not T_STRING)
 * * ERB::Util.html_escape does not allocate a new string when nothing needs to be escaped
 */
static VALUE
erb_escape_html(VALUE self, VALUE str)
{
    if (!RB_TYPE_P(str, T_STRING)) {
        str = rb_convert_type(str, T_STRING, "String", "to_s");
    }

    if (rb_enc_str_asciicompat_p(str)) {
        return optimized_escape_html(str, false);
    }
    else {
        return rb_funcall(rb_cCGI, id_escapeHTML, 1, str);
    }
}

static VALUE
erb_escape_html_bang(VALUE self, VALUE str)
{
    if (!RB_TYPE_P(str, T_STRING)) {
        str = rb_convert_type(str, T_STRING, "String", "to_s");
    }

    if (rb_enc_str_asciicompat_p(str)) {
        return optimized_escape_html(str, true);
    }
    else {
        return rb_funcall(rb_cCGI, id_escapeHTML, 1, str);
    }
}

static VALUE
old_erb_escape_html(VALUE self, VALUE str)
{
    if (!RB_TYPE_P(str, T_STRING)) {
        str = rb_convert_type(str, T_STRING, "String", "to_s");
    }

    if (rb_enc_str_asciicompat_p(str)) {
        return old_optimized_escape_html(str);
    }
    else {
        return rb_funcall(rb_cCGI, id_escapeHTML, 1, str);
    }
}

void
Init_escape(void)
{
#ifdef HAVE_RB_EXT_RACTOR_SAFE
    rb_ext_ractor_safe(true);
#endif

    rb_cERB = rb_define_class("ERB", rb_cObject);
    rb_mEscape = rb_define_module_under(rb_cERB, "Escape");
    rb_define_module_function(rb_mEscape, "html_escape", erb_escape_html, 1);
    rb_define_module_function(rb_mEscape, "html_escape!", erb_escape_html_bang, 1);
    rb_define_module_function(rb_mEscape, "old_html_escape", old_erb_escape_html, 1);

    rb_cCGI = rb_define_class("CGI", rb_cObject);
    id_escapeHTML = rb_intern("escapeHTML");
}
