# Git Branching Strategy

Recommended training-project strategy:

- `main`: stable demo-ready version
- `dev`: integration branch
- `feature/driver`
- `feature/cpp-app`
- `feature/dashboard`
- `feature/testing`
- `docs/final-report`

Typical flow:

```text
feature/* -> dev -> main
```

Commit frequently with descriptive messages such as:
`feat: add ioctl pulse counter`, `feat: add dashboard API`,
`test: add energy calculator tests`, `docs: finalize PRD`.
