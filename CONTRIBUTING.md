# Contributing

Read [AGENTS.md](AGENTS.md), the [modernization roadmap](docs/roadmap.md), the
[native GPU plan](docs/native-gpu-plan.md) and your assigned issue first.
Implementation and new issues belong in `furqanagwan/Re360`; issue numbers in
older commits and documents refer to `furqanagwan/rexglue-sdk`. Upstream projects
are read-only references for this work. The Windows/GDK/D3D12 destination and
regression gates in the [ADRs](docs/adr/README.md) take precedence over inherited
cross-platform guidance. Preserve author/license provenance for selective ports.

Run the relevant unit/PPC, synthetic and title/vendor tests in the issue, record
actual results and blocked cases, and update the documentation with behavior
changes. Do not close a compatibility migration based only on a successful build.

Build, test and formatting commands for this fork are in [README](README.md)
and [AGENTS](AGENTS.md); supported configurations and their evidence are in
[release evidence](docs/release-evidence.md). The upstream wiki below predates
this fork's Windows-only scope; where they differ, this repository's documents
win. See the upstream [Contributing Guide](https://github.com/rexglue/rexglue-sdk/wiki/Development/Contributing) for:

- Build prerequisites and setup
- Code style conventions
- Formatting and linting instructions
- Git setup (line endings, rebasing)
- PR submission workflow

The current module ownership, C++/ABI conventions, tooling and shared human/AI
workflow are in [development guidance](docs/development.md). GDK is required;
the canonical preset is `win-amd64-gdk`, with the edition pinned to 260404.
Use the issue's actual acceptance checks and report blocked gates. Current
representative title testing covers the three 007 games; shared SDK work is
prioritized and batched before expensive title rebuilds.
