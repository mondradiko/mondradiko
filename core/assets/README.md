# Assets

The asset classes inside this directory are responsible for loading asset data
from asset bundles at runtime, and providing an interface for the rest of the
engine to use.

## Asset Class

`Asset.h` contains the definition for the base `Asset` class, which all asset
classes inherit from. It provides the `load()` abstract method, which each
derived class is responsible for implementing. It also has the `isLoaded()`
virtual method, which can be overridden by derived classes to change their
loading functionality, i.e. when a `MaterialAsset` is only considered loaded
when all of its referenced `TextureAsset`s are also loaded.

Note: The use of virtual/abstract methods is not particularly relevant to
performance, as each virtual method is expected to only be called a few times
over the lifetime of that instance.

## Asset Handles

`AssetHandle.h` defines a template class for handling referencing to instances
of classes derived from `Asset`. `AssetHandle`s automatically reference-count
the assets they're handling, and provide some safety guarantees for operations
on those assets.

## Asset Pool

All assets are loaded from an `AssetPool` instance. Before an asset type can
be loaded, it must first be initialized by `initializeAssetType()`, and passed
the parameters required by that asset type's constructor. Then, `AssetHandle`s
can be created using `load()`.

# To-Do

- Make each engine component responsible for initializing/destroying specific assets
- PrimaryAsset and SecondaryAsset
- Generate texture mips in converter
- EnTT resource cache
- Asynchronous asset loading
