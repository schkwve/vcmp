# VCMP Contributing Guidelines

Thank you for contributing to VCMP! This document outlines our standards for commit messages, code formatting, and dependency management.

## Part 1: Commit Messages
To keep our history clear and consistent, please follow these guidelines for commit messages.

### Format
Each commit message should use the following format:
```
<type>([optional scope]): <description>

[optional body]

[optional footer(s)]
```

### Examples
- `chore(frontend/web): Removed unnecessary comments`
- `feat(core): Added API calls for user authentication`

### Commit Types
Use the following types to categorize commits:

- **feat** – Introduces a new feature.
- **fix** – Fixes a bug.
- **chore** – Changes unrelated to features or fixes (e.g., dependency updates).
- **refactor** – Code restructuring that doesn’t add features or fix bugs.
- **docs** – Documentation updates, like changes to README or markdown files.
- **style** – Code formatting changes that don’t affect functionality (e.g., whitespace).
- **test** – Adds or improves tests.
- **perf** – Enhancements that improve performance.
- **ci** – Continuous integration-related changes.
- **build** – Modifications to the build system or external dependencies.
- **revert** – Reverts a previous commit.

## Part 2: Code Formatting
Consistency in code style is important for readability and maintenance. Follow these guidelines:

- Use the provided `clang-format` file to maintain consistent code formatting across the project.
- For documentation or files not supported by `clang-format`, use 4 spaces for indentation.
- **Note:** Ensure you run `clang-format` before committing code changes.

## Part 3: Dependencies
Dependencies may be added if they meet the following criteria:

- **License compatibility:** All dependencies must be compatible with the MIT license.
- **Integration:** Link dependencies in `CMakeLists.txt` using `find_package` (e.g., `find_package(Libevent REQUIRED)`). If `pkg-config` doesn’t work, you may add the library as a Git submodule.

Thank you for helping make VCMP better! If you have questions, please reach out to the maintainers.
