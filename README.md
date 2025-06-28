# CMake-ProjectTemplate
Basic CMake template I use for all my projects

To use this:
1. add as a remote into the repo (say `project_template`)
1. create a new branch (say `template/master`) pegged to this remote
1. run `git config remote.project_template.push refs/heads/template/master:refs/heads/master` so git push will push from project_template to this remote
1. run `git branch --set-upstream-to=project_template/master template/master` so a git pull will update from this remote
1. merge `template/master -> master` with `--unrealated-histories` so that the structure is setup

Now the structure is setup, from now on just pull and just merge `template/master->master` to pull in updates to the project structure