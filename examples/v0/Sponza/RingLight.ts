// Compile with:
// $ asc -t SlidingLight.wat -O3 SlidingLight.ts

import Entity from "./components/Entity";
import PointLightComponent from "./components/PointLightComponent";
import World from "./components/World";

function clamp(x: f64, lower: f64, upper: f64): f64 {
  if (x < lower)
    x = lower;
  if (x > upper)
    x = upper;
  return x;
}

function smoothstep(x: f64): f64 {
  x = clamp(x, 0.0, 1.0);
  return x * x * (3 - 2 * x);
}

class Color {
  constructor(public r: f64, public g: f64, public b: f64) {}
}

function HSVtoRGB(h: f64, s: f64, v: f64): Color {
  if (s == 0.0) return new Color(v, v, v);

  let i = Math.floor(h * 6.0);
  let f = (h * 6.0) - i;
  let p = v * (1.0 - s);
  let q = v * (1.0 - s * f);
  let t = v * (1.0 - s * (1.0 - f));

  i %= 6;

  if (i == 0) return new Color(v, t, p);
  if (i == 1) return new Color(q, v, p);
  if (i == 2) return new Color(p, v, t);
  if (i == 3) return new Color(p, q, v);
  if (i == 4) return new Color(t, p, v);
  if (i == 5) return new Color(v, p, p);

  return new Color(1.0, 0.0, 1.0);
}

export class RingLight {
  lights: Array<Entity> = new Array<Entity>(0);
  hue_offset: f64 = 0.0;

  constructor(public radius: f64, public delta: f64) {
    let theta = 0.0;
    while (theta < Math.PI * 2.0) {
      let x = Math.sin(theta) * radius;
      let y = 0.05;
      let z = Math.cos(theta) * radius;

      let light_entity = World.spawnEntityAt(x, y, z);
      light_entity.addPointLight().setIntensity(1.0, 0.0, 1.0);

      this.lights.push(light_entity);
      theta += delta;
    }
  }

  update(dt: f64): void {
    this.hue_offset += dt * 0.1;
    let theta = 0.0;
    for (let i = 0; i < this.lights.length; i++) {
      let point_light = this.lights[i].getPointLight();
      let hue = (theta / (Math.PI * 2.0)) + this.hue_offset;
      let color = HSVtoRGB(hue, 1.0, 1.0);
      point_light.setIntensity(color.r, color.g, color.b);
      theta += this.delta;
    }
  }
}

// Dummy update() for now until we have script class instantiation
let ring_light = new RingLight(1.0, 0.3);

// self is meant to represent the entity ID,
// and we can just treat it like a Transform since the core API
// uses the entity ID as each method's instance reference
// (passed as i32)
export function update(self: Entity, dt: f64): void {
  ring_light.update(dt);
}
