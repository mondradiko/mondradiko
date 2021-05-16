// Compile with:
// $ asc -t SlidingLight.wat -O3 --exportRuntime SlidingLight.ts

import Entity from "./components/Entity";

export class SlidingLight {
  speed: f64 = 3.0;
  destination_a: f64 = -10.0;
  destination_b: f64 = 10.0;

  velocity: f64;

  constructor(public entity: Entity) {
    this.velocity = this.speed;
  }

  update(dt: f64): void {
    let transform = this.entity.getTransform();

    let x = transform.getX();
    let y = transform.getY();
    let z = transform.getZ();

    if (x < this.destination_a) {
      this.velocity = this.speed;
    } else if(x > this.destination_b) {
      this.velocity = -this.speed;
    }

    x += this.velocity * dt;

    transform.setPosition(x, y, z);
  }
}
