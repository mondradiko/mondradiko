// To compile:
// $ asc -b ui_script.wasm -O3 ui_script.ts

import UiPanel from "../builddir/codegen/ui/UiPanel";

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

class PanelImpl {
  hue_offset: f64 = 0.0;

  transition_step: f64;
  transition_time: f64 = 0.25;
  target_width: f64;
  target_height: f64;

  constructor(public panel: UiPanel) {
    this.transition_step = 0.0;
    this.target_width = panel.getWidth();
    this.target_height = panel.getHeight();
    panel.setSize(0.0, 0.0);
  }

  update(dt: f64): void {
    this.transition_step += dt;

    if (this.transition_step < this.transition_time) {
      let lerp: f64 = smoothstep(this.transition_step / this.transition_time);
      this.panel.setSize(this.target_width * lerp, this.target_height * lerp);
    } else {
      this.hue_offset += dt;
      let r = Math.sin(this.hue_offset) * 0.5 + 0.5;
      let g = 0.0;
      let b = Math.cos(this.hue_offset) * 0.5 + 0.5;
      this.panel.setColor(r, g, b, 0.9);

      if (this.transition_step > 3.0) {
        this.transition_step = 0.0;
      }
    }
  }
}

let this_panel: PanelImpl;

export function createPanel(new_panel: UiPanel): PanelImpl {
  this_panel = new PanelImpl(new_panel);
  return this_panel;
}

export function update(dt: f64): void {
  this_panel.update(dt);
}
