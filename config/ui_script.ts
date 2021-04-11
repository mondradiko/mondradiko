// To compile:
// $ asc -b ui_script.wasm -O3 ui_script.ts

import GlyphStyle from "../builddir/codegen/ui/GlyphStyle";
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

class PanelImpl {
  hue_offset: f64 = 0.0;

  style: GlyphStyle;

  transition_step: f64;
  transition_time: f64 = 0.25;
  target_width: f64;
  target_height: f64;

  constructor(public panel: UiPanel) {
    this.transition_step = 0.0;
    this.target_width = panel.getWidth();
    this.target_height = panel.getHeight();
    panel.setSize(0.0, 0.0);

    this.style = panel.createGlyphStyle();
    this.style.setColor(1.0, 0.0, 1.0, 1.0);
    this.style.setOffset(0.0, 0.0);
  }

  update(dt: f64): void {
    this.transition_step += dt;

    let lerp = smoothstep(this.transition_step / this.transition_time);
    this.panel.setSize(this.target_width * lerp, this.target_height * lerp);

    this.hue_offset += dt;
    let panel_color = HSVtoRGB(this.hue_offset / 3.0, 0.8, 0.1);
    this.panel.setColor(panel_color.r, panel_color.g, panel_color.b, 0.9);

    this.style.setOffset(Math.sin(this.hue_offset) * 0.45, 0.0);

    if (this.transition_step > 3.0) {
      this.transition_step = 0.0;
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
