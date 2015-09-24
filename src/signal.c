#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h> 
#include <caml/threads.h> 
#include <caml/callback.h>
#include <caml/fail.h>
#include <caml/unixsupport.h>
#include <caml/custom.h>

extern int caml_convert_signal_number(int signo);
extern int caml_rev_convert_signal_number(int signo);

value eunix;

CAMLprim value signal_initialize() {
  CAMLparam0();
  eunix = caml_hash_variant("EUnix");
  CAMLreturn(Val_unit);
}

int sigset_compare(value v1, value v2) {
  return memcmp(Data_custom_val(v1), Data_custom_val(v2), sizeof(sigset_t));
}

static struct custom_operations sigset_custom_ops = {
  identifier:  "Posix_signal.sigset",
  finalize:    custom_finalize_default,
  compare:     sigset_compare,
  hash:        custom_hash_default,
  serialize:   custom_serialize_default,
  deserialize: custom_deserialize_default
};

static value copy_sigset(sigset_t *sset) {
  CAMLparam0();
  CAMLlocal1(v);
  v = caml_alloc_custom(&sigset_custom_ops, sizeof(sigset_t), 0, 1);
  memcpy(Data_custom_val(v), sset, sizeof(sigset_t));
  CAMLreturn(v);
}

CAMLprim value signal_sigemptyset(value unit) {
  CAMLparam1(unit);
  sigset_t sset;

  caml_release_runtime_system();
  sigemptyset(&sset);
  caml_acquire_runtime_system();

  CAMLreturn(copy_sigset(&sset));
}

CAMLprim value signal_sigfillset(value unit) {
  CAMLparam1(unit);
  sigset_t sset;

  caml_release_runtime_system();
  sigfillset(&sset);
  caml_acquire_runtime_system();

  CAMLreturn(copy_sigset(&sset));
}

CAMLprim value signal_sigaddset(value set, value sig) {
  CAMLparam2(set, sig);
  CAMLlocal2(result, perrno);
  int lerrno, rc, csig;
  sigset_t sset;

  csig = caml_convert_signal_number(Int_val(sig));
  memcpy(&sset, Data_custom_val(set), sizeof(sigset_t));

  caml_release_runtime_system();
  rc = sigaddset(&sset, csig);
  lerrno = errno;
  caml_acquire_runtime_system();

  if (-1 == rc) {
    goto ERROR;
  }

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, copy_sigset(&sset));
  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(lerrno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

CAMLprim value signal_sigdelset(value set, value sig) {
  CAMLparam2(set, sig);
  CAMLlocal2(result, perrno);
  int lerrno, rc, csig;
  sigset_t sset;

  csig  = caml_convert_signal_number(Int_val(sig));
  memcpy(&sset, Data_custom_val(set), sizeof(sigset_t));

  caml_release_runtime_system();
  rc = sigdelset(&sset, csig);
  lerrno = errno;
  caml_acquire_runtime_system();

  if (-1 == rc) {
    goto ERROR;
  }

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, copy_sigset(&sset));
  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(lerrno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

CAMLprim value signal_sigismember(value set, value sig) {
  CAMLparam2(set, sig);
  CAMLlocal2(result, perrno);
  int lerrno, rc, csig;
  sigset_t sset;

  csig = caml_convert_signal_number(Int_val(sig));
  memcpy(&sset, Data_custom_val(set), sizeof(sigset_t));

  caml_release_runtime_system();
  rc = sigismember(&sset, csig);
  lerrno = errno;
  caml_acquire_runtime_system();

  if (-1 == rc) {
    goto ERROR;
  }

  result = caml_alloc(1, 0); // Result.Ok
  Store_field(result, 0, Val_bool(rc));

  goto END;

ERROR:
  perrno = caml_alloc(2, 0);
  Store_field(perrno, 0, eunix); // `EUnix
  Store_field(perrno, 1, unix_error_of_code(lerrno));

  result = caml_alloc(1, 1); // Result.Error
  Store_field(result, 0, perrno);

END:
  CAMLreturn(result);
}

