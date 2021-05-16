// To compile:
// $ asc -b ui_script.wasm -O3 --exportRuntime ui_script.ts

import GlyphStyle from "../builddir/codegen/ui/GlyphStyle";
import UiPanel from "../builddir/codegen/ui/UiPanel";
import Vector2 from "../builddir/codegen/types/Vector2";

class QueuedMessage {
  lifespan: f64 = 5.0;
  fade_duration: f64 = 0.25;

  max_height: f64 = 0.05;
  height: f64 = this.max_height;

  constructor(public text: string, public style: GlyphStyle, public offset_x: f64, offset_y: f64) {
    this.style.setColor(1.0, 1.0, 1.0, 1.0);
    this.style.setText(this.text);
    this.setOffset(offset_y);
  }

  update(dt: f64): bool {
    this.lifespan -= dt;

    if (this.lifespan < this.fade_duration) {
      let alpha: f64 = this.lifespan / this.fade_duration;
      this.style.setColor(1.0, 1.0, 1.0, alpha);
      this.height = this.lifespan * this.max_height / this.fade_duration;
    }

    return this.lifespan > 0.0;
  }

  setOffset(offset_y: f64): void {
    this.style.setOffset(this.offset_x, offset_y);
  }
}

function drawCircle(panel: UiPanel,
                    x: f64, y: f64, radius: f64,
                    r: f64, g: f64, b: f64, alpha: f64): void {
  panel.drawCircle(x, y, radius, r, g, b, alpha);
}

class PolkaDot {
  lifespan: f64 = 0.5;
  age: f64 = this.lifespan;
  grow_duration: f64 = 0.05;
  shrink_duration: f64 = 0.25;
  fade_duration: f64 = 0.05;

  constructor(public x: f64, public y: f64, public radius: f64, public r: f64, public g: f64, public b: f64) {}

  update(dt: f64, panel: UiPanel): bool {
    this.age -= dt;

    if (this.age < 0.0) return false;

    let radius = this.radius;
    if (this.age < this.shrink_duration) {
      radius *= this.age / this.shrink_duration;
    } else if (this.age > this.lifespan - this.grow_duration) {
      radius *= (this.lifespan - this.age) /
                (this.lifespan - this.grow_duration);
    }

    let alpha = 1.0;
    if (this.age < this.fade_duration) {
      alpha *= this.age / this.fade_duration;
    }

    drawCircle(panel, this.x, this.y, radius, this.r, this.g, this.b, alpha);

    return true;
  }
}

let main_panel: PanelImpl;

export class PanelImpl {
  style_pool: Array<GlyphStyle> = [];
  message_queue: Array<QueuedMessage> = [];
  log_bottom: f64 = -0.45;
  last_message: f64 = 0.0;
  polka_dot_queue: Array<PolkaDot> = [];
  last_polka_dot: f64 = 0.0;

  constructor(public panel: UiPanel) {
    main_panel = this;
  }

  onHover(x: f64, y: f64): void {}

  onSelect(x: f64, y: f64): void {
    let polka_dot = new PolkaDot(x, y, 0.05, 1.0, 1.0, 1.0);
    this.polka_dot_queue.push(polka_dot);

    let e = 1000.0;
    let rx = Math.round(x * e) / e;
    let ry = Math.round(y * e) / e;

    let message_text = `Selected at: (${rx}, ${ry})`;
    this.handleMessage(message_text);
  }

  onDrag(x: f64, y: f64): void {}

  onDeselect(x: f64, y: f64): void {}

  handleMessage(message: string): void {
    let style: GlyphStyle;

    if (this.style_pool.length > 0) {
      style = this.style_pool.pop();
    } else {
      style = this.panel.createGlyphStyle();
    }

    let offset_x = -0.45 * this.panel.getWidth();

    let new_message = new QueuedMessage(message, style, offset_x, this.log_bottom);
    this.message_queue.push(new_message);
    this.log_bottom += new_message.height;

    this.last_message += 0.5;
    if (this.last_message > 0.0) {
      if (this.last_message > 5.0) {
        this.last_message = 5.0;
      }

      new_message.lifespan += this.last_message;
    }
  }

  update(dt: f64): void {
    this.last_polka_dot -= dt;

    while (this.last_polka_dot < 0.0) {
      let x = Math.random() - 0.5;
      let y = Math.random() - 0.5;

      x *= this.panel.getWidth() * 0.95;
      y *= this.panel.getHeight() * 0.95;

      let polka_dot = new PolkaDot(x, y, 0.01, 0.8, 0.0, 0.8);
      this.polka_dot_queue.push(polka_dot);

      this.last_polka_dot += 0.002;
    }

    this.last_message -= dt;

    this.log_bottom = -0.45;

    let i: i32 = 0;

    while (i < this.message_queue.length) {
      let message = this.message_queue[i];

      if (message.update(dt)) {
        this.log_bottom += message.height;
        message.setOffset(this.log_bottom);
        i++;
      } else {
        message.style.setText("");
        this.style_pool.push(message.style);
        this.message_queue.splice(i, 1);
      }
    }

    i = 0;

    while (i < this.polka_dot_queue.length) {
      let polka_dot = this.polka_dot_queue[i];

      if (polka_dot.update(dt, this.panel)) {
        i++;
      } else {
        this.polka_dot_queue.splice(i, 1);
      }
    }
  }
}

export function handleMessage(message: string): void {
  main_panel.handleMessage(message);
}
