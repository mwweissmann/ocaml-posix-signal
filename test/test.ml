open Posix_signal

let ( >>= ) a f = match a with | Result.Ok v -> f v | Result.Error _ as e -> e

let _ =
  let s = sigemptyset () in
  assert(
    ((sigaddset s Sys.sigterm) >>= (fun s -> sigismember s Sys.sigterm))
    = Result.Ok true);
  assert(
    ((sigaddset s Sys.sigfpe) >>= (fun s -> sigismember s Sys.sigterm))
    = Result.Ok false);
  print_endline "o.k."

let _ =
  let s1 = sigemptyset () in
  let s2 = sigemptyset () in
  let s3 = sigfillset () in
  assert(s1 = s2);
  assert(s1 != s3);
  assert(s2 != s3);
  print_endline "o.k."

let _ =
  let s = sigfillset () in
  assert(sigismember s Sys.sigterm = Result.Ok true);
  assert(sigismember s Sys.sigfpe = Result.Ok true)

let _ = print_endline "o.k."

