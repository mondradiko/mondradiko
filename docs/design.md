---
layout: default
title: Design
permalink: /design
---

# High-level Engine Architecture

Mondradiko is split into two main runtimes: the **server** and the **client**.
Servers host the engine world, including scripting, database management,
and physics. Clients connect to the server, which sends the clients all of the
data of the engine world, including data about other clients, so that each
client may render avatars and hear voice chat. Each client also sends the server
its input data, such as headset and controller tracking data, which the server
uses in its game logic.

# Client Types

There are three types of clients: **players**, **spectators**, and **queries**.

## Player Clients

Players are clients that have a VR headset, and can interact with the server,
have an avatar, and can speak into voice chat.

## Spectator Clients

Spectators are pancake mode (non-VR) clients that can fly around in the server,
while unable to interact with any game logic. Spectators do not have avatars,
so they cannot be seen by players or other spectators. However, they may hear
voice chat. Spectators are useful for seeing into the server world so that it
may be live-streamed, for instance.

## Query Clients

Queries are single-time command-line clients that connect to a server, and fetch
its metadata, such as its name, description, website, how many players are
connected, etc.

# Future-proofing

# Optimizing for VR

## Rendering

## Interaction

## Mobile (Oculus Quest)

# Networking

# Core Engine Components

## Entrypoint

## Filesystem

## Assets

## Components

## Entities

## Scene

## Renderer

## Pipelines

## Displays

## Viewports

##  GPU Resources
