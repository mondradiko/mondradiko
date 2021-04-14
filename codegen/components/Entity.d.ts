// Mondradiko scripting API - AssemblyScript bindings
// https://mondradiko.github.io/

import TransformComponent from "./TransformComponent";

@unmanaged declare class Entity {
  @external("Entity_getComponent")
  get<T>(): T

  /////////////////////
  // TransformComponent
  /////////////////////
  @external("Entity_hasTransform")
  hasTransform(): bool

  @external("Entity_addTransform")
  addTransform(): TransformComponent
}

export default Entity;
