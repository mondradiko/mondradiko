# Mondradiko Contribution Guidelines

## Finding Work To Do

To-do lists, along with other important information about the structure of the
engine for code contributors, are kept in README files in various directories
in the repo. Each to-do item will explain what needs done, and can also provide
some links to other helpful resources.

To contribute, please first commit your changes to a new branch, on your own
fork of the upstream repository if applicable, then open a pull request to the
main branch of the upstream repository. Your branch will have to pass
[the pull request requirements](/.github/pull_request_template.md) before we can
merge it.

If you have any questions or need any help, the best place to talk to other
contributors is on [our Discord server](https://discord.gg/NENngxc)!

## Making Suggestions

## Writing Documentation

Documentation contributions are always welcome! Please see
[the guide on writing documentation](/docs/Writing_Documentation.md) for more
details.

## Code Style

When writing source code for Mondradiko, please follow the
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html),
with a couple exceptions:

- For convenience's sake, please use #pragma once over include guards.
- Forward declarations are preferred whenever possible, to decrease compilation time.

GitHub will automatically check your formatting when you make a pull request.

# To-Do

- Flesh this out
