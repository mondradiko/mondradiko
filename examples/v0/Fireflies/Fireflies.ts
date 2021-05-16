// Compile with:
// $ asc -t Fireflies.wat -O3 --exportRuntime Fireflies.ts

import Entity from "../bindings/components/Entity";
import Vector3 from "../bindings/types/Vector3";

export class Firefly {
  static roam_bounds: Vector3 = new Vector3(0.5, 0.3, 0.5);

  phase: f64;
  phase_vec: Vector3;
  phase_offset: Vector3;
  phase_factor: Vector3;
  position: Vector3;
  speed: f64 = 0.3;
  origin: Vector3;

  constructor(public entity: Entity) {
    let transform = this.entity.getTransform();
    let x = transform.getX();
    let y = transform.getY();
    let z = transform.getZ();
    this.origin = new Vector3(x, y, z);

    this.phase = Math.random() * 100.0;

    this.phase_vec = new Vector3(0.0, 0.0, 0.0);

    this.phase_offset = Vector3.random();
    this.phase_offset.mul(new Vector3(100.0, 100.0, 100.0));

    this.phase_factor = Vector3.random();
    this.phase_factor.mul(new Vector3(0.7, 0.7, 0.7)).add(new Vector3(.65, .65, .65));

    this.entity.addPointLight().setIntensity(1.0, 0.6, 0.3);

    this.position = this.origin.clone();
  }

  update(dt: f64): void {
    this.phase += dt;

    let phase = this.phase * this.speed;

    let phase_vec = this.phase_vec;
    phase_vec.x = phase;
    phase_vec.y = phase;
    phase_vec.z = phase;
    phase_vec.add(this.phase_offset).mul(this.phase_factor);

    let pos = this.position;
    pos.x = Math.sin(phase_vec.x);
    pos.y = Math.sin(phase_vec.y);
    pos.z = Math.sin(phase_vec.z);

    pos.mul(Firefly.roam_bounds).add(this.origin);

    let transform = this.entity.getTransform();
    transform.setPosition(pos.x, pos.y, pos.z);
  }
}

export class Fireflies {
  num: i32 = 10;

  center: Vector3 = new Vector3(-0.5, 0.45, -0.5);
  size: Vector3 = new Vector3(10.0, 0.4, 1.8);

  constructor(public entity: Entity) {
    this.entity.getTransform().setPosition(this.center.x, this.center.y, this.center.z);

    let scale_vector: Vector3 = new Vector3(2.0, 2.0, 2.0);
    let offset_vector: Vector3 = new Vector3(1.0, 1.0, 1.0);

    this.size.sub(Firefly.roam_bounds);

    for (let i = 0; i < this.num; i++) {;
      let random_vec: Vector3 = Vector3.random();
      random_vec.mul(scale_vector).sub(offset_vector);
      random_vec.mul(this.size)

      this.entity.spawnScriptedChildAt("Firefly", random_vec.x, random_vec.y, random_vec.z);
    }
  }

  update(dt: f64): void {}
}
