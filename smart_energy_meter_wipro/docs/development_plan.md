# Development Plan

| Stage | Work | Output |
|---|---|---|
| 1 | Introduction | objective, scope |
| 2 | Requirements | PRD, requirements, timeline |
| 3 | Architecture | component, sequence, class, state diagrams |
| 4 | Prototype | driver, C++ app, calculations |
| 5 | Integration | dashboard, logging, tests |
| 6 | Finalization | demo, report, presentation |

The driver-loading issue is environment-specific. The project therefore contains
both a real-driver path and a simulator path so the software can be demonstrated
even when the WSL kernel cannot accept the module.
