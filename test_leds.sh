#!/bin/bash

# Build the project first
cd led-controller
cargo build --release
APP="./target/release/led-controller"

echo "Starting LED test loop using led-controller internal mode..."
$APP --test