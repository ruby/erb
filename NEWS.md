# Change Log

## Unreleased (to be 3.0.0)

- Remove `safe_level` and further positional arguments from `ERB.new`
- Fix a bug that a magic comment with a wrong format could be detected

## 2.2.3

- Bump `required_ruby_version` from 2.3 to 2.5 as it has never been supported

## 2.2.2

- `ERB.version` returns just a version number
- `ERB::Revision` is deprecated

## 2.2.1

- `ERB#initialize` warns `safe_level` and later arguments even without -w

## 2.2.0

- Ruby 3.0 promoted ERB to a default gem
