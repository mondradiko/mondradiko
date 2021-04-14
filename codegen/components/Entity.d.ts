// Mondradiko scripting API - AssemblyScript bindings
// https://mondradiko.github.io/

import PointLightComponent from "./PointLightComponent";
import TransformComponent from "./TransformComponent";

@unmanaged declare class Entity {
  @external("Entity_getComponent")
  get<T>(): T

  /////////////////////
  // PointLightComponent
  /////////////////////
  @external("Entity_hasPointLight")
  hasPointLight(): bool

  @external("Entity_addPointLight")
  addPointLight(): PointLightComponent

  /////////////////////
  // TransformComponent
  /////////////////////
  @external("Entity_hasTransform")
  hasTransform(): bool

  @external("Entity_addTransform")
  addTransform(): TransformComponent
}

export default Entity;
