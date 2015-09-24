external signal_initialize : unit -> unit = "signal_initialize"
let () = signal_initialize ()

type sigset

external sigemptyset : unit -> sigset = "signal_sigemptyset"

external sigfillset : unit -> sigset = "signal_sigfillset"

external sigaddset : sigset -> int -> (sigset, [>`EUnix of Unix.error]) Result.result = "signal_sigaddset"

external sigdelset : sigset -> int -> (sigset, [>`EUnix of Unix.error]) Result.result = "signal_sigdelset"

external sigismember : sigset -> int -> (bool, [>`EUnix of Unix.error]) Result.result = "signal_sigismember"

