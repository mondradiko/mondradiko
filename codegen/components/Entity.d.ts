// Mondradiko scripting API - AssemblyScript bindings
// https://mondradiko.github.io/

import PointLightComponent from "./PointLightComponent";
import TransformComponent from "./TransformComponent";

@unmanaged declare class Entity {
  @external("Entity_spawnChild")
  spawnChild(): Entity

  @external("Entity_spawnChildAt")
  spawnChildAt(x: f64, y: f64, z: f64): Entity

  @external("Entity_spawnScriptedChild")
  spawnScriptedChild(script_impl: string): Entity

  @external("Entity_spawnScriptedChildAt")
  spawnScriptedChildAt(script_impl: string, x: f64, y: f64, z: f64): Entity

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
