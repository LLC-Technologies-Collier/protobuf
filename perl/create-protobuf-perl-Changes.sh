#!/bin/bash

out="/tmp/protobuf-perl/Changes"
mkdir -p "$(dirname "$out")"
echo "" > "$out" # Clear the file before appending

# Get commits since v33.2 in reverse chronological order (newest first)
commits=($(git log --format=%H v33.2..HEAD))

num_commits=${#commits[@]}
version=$num_commits

for commit in "${commits[@]}"
do
  date=$(git show -s --format=%cd --date=short "$commit")
  v="$(printf "33.2.%02d" $version)"
  echo "$v: $date $commit"
  printf "## %s\t%s\n\n" "$v" "$date" >> "$out"
  git show -s "$commit" >> "$out"
  printf "\n\n" >> "$out" # Add some space between entries
  version=$((version - 1))
done
echo "Changelog written to $out"
