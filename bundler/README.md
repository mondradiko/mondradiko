# Asset Bundler

The Mondradiko asset bundler, or just "the bundler" for short, is a command-line
tool for creating asset bundles. It is intended as a replacement for a graphical
editor until a more traditional editor can be developed, and will be what
content creators actually use to create content until such a graphical editor
is created.

## Usage

## Bundler Manifest

The settings for a bundler project are located in the "bundler manifest," which
is a TOML file containing all the assets used that will be bundled, the prefabs
loaded at runtime, and various other settings for asset file conversion. Here's
an example bundler manifest:

```toml
[bundle]
name = "Mondradiko Example World: Sponza"
compression = "LZ4"

[[assets]]
file = "model/Sponza.gltf"
initial_prefab = true

[[assets]]
file = "avatar/example-avatar.vrm"
initial_prefab = true

[[assets]]
file = "scripts/test_as.wat"
alias = "test_script"

[[prefabs]]
initial_prefab = true

  [prefabs.point_light]
  position = [0.0, 1.5, 0.0]
  intensity = [100.0, 0.0, 100.0]

  [prefabs.script]
  script_asset = "test_script"

  [prefabs.transform]
  position = [0.0, 0.0, 0.0]
```

The manifest contains the bundle's settings, the assets it bundles, and a list
of manually-created prefabs.

### Bundle Settings

### Bundled Assets

### Manual Prefabs

## To-Do

- Move converter classes to dedicated converter/ directory and library
- Add bundler cache for incremental bundling
- Write more here about the key parts of the bundler
