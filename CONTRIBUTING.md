# Contributing to OpenHornet-Software

OpenHornet-Software uses **trunk-based development**. There is one long-lived branch (`main`) that always represents a healthy, reviewable state of the project. All contributors create short-lived branches directly from `main`, keep their changes small and focused, and merge them back into `main` via a Pull Request as quickly as practical. There is no `develop` staging branch.

For code writing, documentation (Doxygen), and style standards, see the [Software Manual](https://jrsteensen.github.io/OpenHornet-Software/d7/d78/md__software_manual.html).

---

## GitHub Process

1. **Fork the OH repo**
   1. Click on the `Fork` button in the top right corner of the project's GitHub page.
      - This creates a fork of the repository under your GitHub account that you can modify at will.

2. **Clone the OH repo**
   - The first time, you will need to install the GitHub Desktop app, open it, and log in with your GitHub account info. (If you know how to use CLI Git, this guide is probably too basic for you.)
   1. Open the GitHub Desktop App. Click `File` > `Clone Repository`.
   2. Click on the OpenHornet-Software fork under your account and pick an empty folder for it to clone into.
   3. Click `Clone`.
      - Now you have a copy of the OpenHornet-Software project files on your PC.

3. **Select the base branch**
   1. Click on `Current branch` at the top, select the `main` branch.

4. **Create a branch**
   1. Click on `Current branch` at the top, click `New Branch`, and create a branch.
      - Make sure it's descriptive and follows the guidelines below.

5. **Make the change**
   - Navigate to the folder you cloned OH to, then make the change(s) you want to contribute back to OpenHornet.

6. **Commit the change**
   1. After completing your desired change, go back to the GitHub Desktop app and complete the commit title and description fields.
      - Title should be brief, description should be verbose. Commit frequently so you have lots of points to roll back to if something doesn't work out.

7. **Push the change**
   1. Click the button that displays `Publish branch` to sync changes to GitHub.
      - Now you have the same copy of this branch on your computer as well as on GitHub.

8. **Submit a Pull Request**
   - This is where you really document the nitty gritty of what you did and why you did it. A Pull Request notifies the project maintainers that you have some work that they should review and eventually add to the project.
   1. Go to the main page of your fork on GitHub and click on the highlighted PR message with a green button displaying `Compare & pull request`.
   2. Write a title and in the comments section summarize the work you did.
      - Use our template that shows when you create a PR.
      - Be verbose.
      - Be detailed.
      - Add pictures describing your change.
      - Tell us why it changed.
      - Reference an issue.
      - Use the checklist in the PR.
   3. Click `Create pull request` and leave the box checked that says `Allow edits from maintainers`.

---

## Aw crap, I need to fix my PR (Or a reviewer requested changes.)

That's easy! Just make another commit against the same branch the PR is against and it will apply it to the PR. (Try not to open a new PR unless explicitly instructed to by repository maintainers.)

---

## Repository Structure

```
main  (trunk — the single long-lived branch)
├── feature-new_sketch-1A2_MASTER_ARM_PANEL
├── bug-typo-1A1A1_RS485_BUS_MASTER_UIP
├── docs-style_guide
└── maint-refactor-2A13_ABSIS_BACKLIGHT_CONTROLLER
```

---

## Branches

### Trunk branch (`main`)

The `main` branch is the **single long-lived branch** for the repository.

- It is the shared integration point for all ongoing work and must always remain in a healthy, compilable, reviewable state.
- Contributors **SHALL NOT** commit directly to `main` without explicit admin direction.
- All Pull Requests target `main`.
- On push or merge to `main`: CI compiles all sketches and Doxygen documentation is generated and deployed to [GitHub Pages](https://jrsteensen.github.io/OpenHornet-Software/).

> **Warning** Individual contributors should NEVER commit, push, or merge to `main` without explicit admin approval.

### Short-lived contribution branches

All contribution branches are created from `main` and must be kept **short-lived** — merged back to `main` as soon as the work is complete and reviewed.

> Keep your branches **short-lived**. A contribution branch should ideally live for hours to a few days, not weeks or months. Break large changes into a series of smaller, self-contained PRs where possible. This reduces merge conflicts and makes review faster and easier.

A properly formed branch name shall consist of:

- the **type** of change followed by a dash (`-`), i.e. `feature-`, `bug-`, `docs-`, or `maint-`;
- a **short description** of the change followed by a dash (`-`), i.e. `new_sketch-`, `refactor-`, `typo-`, etc.;
- the **applicable item or sketch**, with all words separated by an underscore (`_`), i.e. `1A2_MASTER_ARM_PANEL`, `2A13_ABSIS_BACKLIGHT_CONTROLLER`, `1A1A1_RS485_BUS_MASTER_UIP`.
  - Reference designators should be selected from the interconnect drawing, with a short descriptive name after it.

`feature-new_sketch-1A2_MASTER_ARM_PANEL` is a valid feature branch name. `bug-new_sketch-RIGHTDDI` would not be valid because it doesn't follow the naming convention, and you wouldn't create a brand new sketch for a bug fix.

All development branches **SHALL** PR into `main`.

#### Feature branch type

Feature branches are descriptively named branches for stand-alone new features that will not/cannot affect dependencies. They shall PR into the `main` branch.

#### Bugfix branch type

Bugfix branches resolve identified issues with an OH component. They may affect dependencies. They shall PR into the `main` branch.

#### Docs branch type

Documentation branches create or update project documentation and should not affect any hardware component. Doc updates should be included directly with a feature or bugfix update; a docs branch should only be created if that situation does not apply. They shall PR into the `main` branch.

#### Maint branch type

Maintenance branches are for cleanup, refactoring, quality-of-life updates, and other issues not rising to the level of a bug. They shall PR into the `main` branch.

---

## Commits

### Commit Frequency

Commits shall be pushed to remote (the OH GitHub Repo) at least daily on any day work has taken place, but preferably pushed as each commit is made, in real time. This policy will help eliminate overlap and conflicting dependencies.

### Commit Messages

- The commit subject should be capitalized.
- The commit subject should be no more than 50 characters in length (with wiggle room up to 72 characters as a hard max).
- The commit subject should not contain punctuation at the end of it.
- The commit subject should use the imperative mood (i.e. "Clean your room", "Close the door", etc.)
- A properly formed Git commit subject line should always be able to complete the following sentence:
  - If applied, this commit will **your subject line here**
  - For example:
    - If applied, this commit will _Refactor subsystem X for readability_
    - If applied, this commit will _Update getting started documentation_
    - If applied, this commit will _Remove deprecated methods_
    - If applied, this commit will _Release version 1.0.0_
    - If applied, this commit will _Merge pull request #123 from user/branch_
- The commit message body should use proper capitalization and punctuation.
- The commit message body should be detailed enough that the reader can understand the changes made.
- The commit message body should reference other issues and pull requests at the end. `Closes: #123` `See also: #456, #789`

---

## Workflows

This section describes the GitHub Actions workflows that run automatically on the repository.

### Continuous Integration (`ci.yaml`)

| Property | Value |
|---|---|
| **Trigger** | All PRs (opened, reopened, synchronized); push to `main` |
| **Manual trigger** | No |

**Function:** Compiles all Arduino sketches in the `embedded/` directory using GNU Make and Arduino IDE. This is a smoke test — it verifies that every sketch actually compiles against its target microcontroller with DCS-BIOS and any other required libraries. On success, compiled `.eep` and `.hex` files are uploaded as a `firmware` artifact (retained for 7 days).

All PRs must pass this workflow before being eligible for merge.

### Generate & Deploy Doxygen Docs (`main-generate-and-deploy-doxygen-docs.yaml`)

| Property | Value |
|---|---|
| **Trigger** | PRs targeting `main`; push to `main`; manual dispatch |
| **Manual trigger** | Yes |

**Function:** Builds HTML Doxygen documentation from `./docs/Doxyfile` and deploys it to the `gh-pages` branch, which powers the public documentation site at [https://jrsteensen.github.io/OpenHornet-Software/](https://jrsteensen.github.io/OpenHornet-Software/).

### Generate Doxygen Docs (`develop-generate-doxygen-docs.yaml`)

| Property | Value |
|---|---|
| **Trigger** | PRs targeting `develop`; push to `develop`; manual dispatch |
| **Manual trigger** | Yes |

**Function:** Builds HTML Doxygen documentation from `./docs/Doxyfile` as a build-only check (no deployment to GitHub Pages).

> **Admin note:** This workflow still references the `develop` branch, which no longer exists as a long-lived branch. Since there are no more pushes to `develop`, this workflow will only fire on manual dispatch. Repository admins should consider retiring this workflow or updating it to serve as a PR-only doc build check targeting `main`.