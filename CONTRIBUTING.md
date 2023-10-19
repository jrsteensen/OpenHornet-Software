
# CONTRIBUTING
This is TBD. Please feel free to propose ideas to flesh this out.

## Contributing to the OpenHornet Repository

### 1) Fork the OH repo
The first step is to fork the repo (repository), which creates a fork of the repository under your GitHub account you can modify at will.
  1. Click on the ```Fork``` button in the top right corner of the project's GitHub page.

### 2) Clone the OH repo
The first time, you will need to install the GitHub desktop app, open it and login with your GitHub account info. (If you know how to use CLI GIT, this guide is probably too basic for you.) 
  1. Open the GitHub Desktop App. Click ```File``` > ```Clone Repository```.
  1. Click on the OpenHornet fork under your account, and pick an empty folder for it to clone into.
  1. Click ```Clone```. 

### 3) Select the base branch
Click on ```Current branch``` at the top, select the branch you want to work with as a base. (Usually, it will be the `develop` branch.)

### 4) Create a branch
Click on ```Current branch``` at the top, click ```New Branch```, and create a branch. Make sure it's descriptive and follows the guidelines below.

### 5) Make the change
Navigate to the folder you cloned OH to, then make the change(s) you want to contribute back to OpenHornet.

### 6) Commit the change
After completing your desired change, go back to GitHub desktop app and complete the commit title and descriptions fields. title should be brief, description should be verbose. Commit frequently, so you have lots of points to roll back to if something doesn't work out.

### 7) Push the change
The next step is to synchronize your local changes with your repository on GitHub. You need to push your changes to GitHub. Click the button that displays ```Publish branch``` to sync changes. Now you have the same copy of this branch on your computer as well as on GitHub.

### 8) Submit a Pull Request
This is where you really document the nitty-gritty of what you did and why you did it. 

A Pull Request notifies the project maintainers that you have some work that they should review and eventually add to the project.

Go to the main page of your fork on GitHub and click on the highlighted PR message with a green button displaying ```Compare & pull request```.

Write a title and in the comments section summarize the work you did. 

* Use our template that shows when you create a PR. 
* Use the checklist in the PR.
* Be verbose. 
* Be detailed. 
* Add pictures describing your change. 
* Tell us why it changed. 
* Reference an issue. 

Click ```Create pull request``` and leave the box checked that says ```Allow edits from maintainers```.

## Aw crap, I need to fix my PR (Or a reviewer requested changes.)
* That's easy! Just make another commit against the same branch the PR is against and it will apply it to the PR. (Try not to open a new PR unless explicitly instructed to by repository maintainers.)

## Branches
OpenHornet uses a basic branch strategy for the software repository. Development work is done in a branch named something like `feature-new_sketch-1A2_MASTER_ARM_PANEL`. When it is complete, the contributing user will create a PR to the `develop` branch. The `develop` branch is used to stage all the changes for the next release. When the software achieves a new release state, it is merged to the `main` branch, which always represents the current stable and tested release.

### Main branch
This branch is for the current release, and should contain stable, tested and known good code only. It should never be committed to directly, outside of an emergency hotfix at repository admin direction. It is merged from the `develop` branch.
* Only repository admin can merge to this branch.
* Must compile doxygen documents which is (hosted here)[https://jrsteensen.github.io/OpenHornet-Software/index.html].
* All Arduino sketches must pass CI testing, which is a simple 'smoke-test' that just verifies it does in fact compile with Arduino IDE, DCS-BIOS and any other required libraries/dependencies on the microcontroller it was designed for.
> **Warning**
> Individual contributors should NEVER commit, push or merge to this branch without explicit admin approval.

### Develop branch
This branch is where code is gathered and documented for the next release. It should contain only code that compiles, and is generally ready for release.
* Only repository admin and maintainers can merge to this branch.
* Must compile doxygen documents which are kept at `/docs/html/index.html` and available from within the repository.
* All Arduino sketches must pass CI testing, which is a simple 'smoke-test' that just verifies it does in fact compile with Arduino IDE, DCS-BIOS and any other required libraries/dependencies.

### Development branches
These branches are where the majority of development happens. 
* A properly consisted branch name shall consist of: 
  * the type of change followed by a dash (`-`), i.e. `feature-`, `bug-`, `docs-`, or `maint-`;
  * a short description of the change followed by a dash (`-`), i.e. `new_sketch-`, `refactor-`, `typo-`, etc.;
  * the applicable item or sketch, with all words separated by an underscore (`_`), i.e. `1A2_MASTER_ARM_PANEL`, `2A13_ABSIS_BACKLIGHT_CONTROLLER`, `1A1A1_RS485_BUS_MASTER_UIP`.
    * Reference designators should be selected from the interconnect drawing, with a short descriptive name after it.
* `feature-new_sketch-1A2_MASTER_ARM_PANEL` is a valid feature branch name.`bug-new_sketch-RIGHTDDI` would not be because it doesn't follow the naming convention and you wouldn't create a brand new sketch for a bug fix.

#### Feature branch type
Feature branch type are descriptively named branches which are stand-alone new features and will not/can not affect dependencies. They shall feed into the version branch it is associated with.

#### Bugfix branch type
Bugfix branch type resolve identified issues with an OH component. They may affect dependencies. Any bugfix shall feed into the version branch it is associated with.

#### Docs branch type
Documentation branch type create or update project documentation, and should not affect any hardware component. Doc updates should be included directly with a feature or bugfix update, and a docs branch should only be created if that situation does not apply. They shall feed into the version branch it is associated with.

#### Maint branch type
Maintenance branch type are for cleanup, refactoring, quality-of-life updates, and other issues not rising to the level of a bug. They shall feed into the version branch it is associated with.

## Commits

### Commit Frequencies
Commits shall be pushed to remote (the OH GitHub Repo) at least daily on any day work has taken place, but preferably pushed as each commit is made, real-time. This policy will help eliminate overlap and conflicting dependencies.

### Commit Messages
* The commit subject should be capitalized.
* The commit subject should be no more than 50 characters in length (with wiggle room up to 72 characters in length as a hard max.) 
* The commit subject should not contain punctuation at the end of it.
* The commit subject should use the imperative mood (i.e. "Clean your room", "Close the door", etc.)
* A properly formed Git commit subject line should always be able to complete the following sentence:
  * If applied, this commit will **your subject line here**
  * For example:
    * If applied, this commit will *Refactor subsystem X for readability*
    * If applied, this commit will *Update getting started documentation*
    * If applied, this commit will *Remove deprecated methods*
    * If applied, this commit will *Release version 1.0.0*
    * If applied, this commit will *Merge pull request #123 from user/branch*
* The commit message body should use proper capitalization and punctuation.
* The commit message body should be detailed enough that the reader can understand the changes made.
* The commit message body should reference other issues and pull requests at the end.
    Resolves: #123
    See also: #456, #789

## Workflows
This section is TBD.
### Main Branch Workflows

##### <Workflow Name>
* **Trigger Conditions:** On push to main branch
* **Can manually trigger?** Yes/No
* **Function:** XXXX
##### <Workflow Name>
* **Trigger Conditions:** On push to main branch
* **Can manually trigger?** Yes/No
* **Function:** XXXX

### Develop Branch Workflows

##### <Workflow Name>
* **Trigger Conditions:** On push to main branch
* **Can manually trigger?** Yes/No
* **Function:** XXXX
##### <Workflow Name>
* **Trigger Conditions:** On push to main branch
* **Can manually trigger?** Yes/No
* **Function:** XXXX