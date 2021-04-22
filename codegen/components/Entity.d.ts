// Mondradiko scripting API - AssemblyScript bindings
// https://mondradiko.github.io/

import PointLightComponent from "./PointLightComponent";
import TransformComponent from "./TransformComponent";

@unmanaged declare class Entity {

  /////////////////////
  // PointLightComponent
  /////////////////////
  @external("Entity_hasPointLight")
  hasPointLight(): bool

  @external("Entity_addPointLight")
  addPointLight(): PointLightComponent

  @external("Entity_getPointLight")
  getPointLight(): PointLightComponent

  /////////////////////
  // TransformComponent
  /////////////////////
  @external("Entity_hasTransform")
  hasTransform(): bool

  @external("Entity_addTransform")
  addTransform(): TransformComponent

  @external("Entity_getTransform")
  getTransform(): TransformComponent
}

export default Entity;
