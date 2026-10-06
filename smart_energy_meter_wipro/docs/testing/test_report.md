# Smart Energy Meter Test Report

## Test scope

The project is tested in layers: calculations, Linux driver/ioctl, C++ integration, HTTP API, dashboard flow and end-to-end operation.

| ID | Test | Expected | Status |
|---|---|---|---|
| T01 | C++ calculation build | Application compiles with C++17 | PASS |
| T02 | Energy calculation | pulses / 1000 = kWh | PASS |
| T03 | Alert threshold | power > 3000 W produces HIGH | PASS |
| T04 | HTTP status | `/api/status` returns JSON | PASS |
| T05 | Pulse API | `/api/pulse?n=N` accepts valid pulse count | PASS |
| T06 | Reset API | `/api/reset` resets counter | PASS |
| T07 | CSV export | `/api/export` returns CSV | PASS |
| T08 | Driver build | `.ko` builds against matching WSL kernel | Environment dependent |
| T09 | Device permissions | normal user can access `/dev/smartmeter` | PASS after `chmod 666` |
| T10 | IOCTL SET/GET/RESET | kernel counter changes correctly | Requires loaded driver |
| T11 | C++ driver integration | C++ reads and resets real device | Requires loaded driver |
| T12 | End-to-end | driver → C++ → analytics → dashboard | Requires loaded driver |

## Important environment note

A kernel module must be compiled against the exact running kernel. If `insmod` reports `Invalid module format` or `module_layout` mismatch, verify:

```bash
uname -r
make -C /lib/modules/$(uname -r)/build kernelrelease
```

The two kernel release strings must match.
