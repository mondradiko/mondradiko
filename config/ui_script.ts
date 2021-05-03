// To compile:
// $ asc -b ui_script.wasm -O3 --exportRuntime ui_script.ts

import GlyphStyle from "../builddir/codegen/ui/GlyphStyle";
import UiPanel from "../builddir/codegen/ui/UiPanel";

class QueuedMessage {
  lifespan: f64 = 5.0;
  fade_duration: f64 = 0.25;

  max_height: f64 = 0.05;
  height: f64 = this.max_height;

  constructor(public text: string, public style: GlyphStyle, offset: f64) {
    this.style.setColor(1.0, 1.0, 1.0, 1.0);
    this.style.setText(this.text);
    this.setOffset(offset);
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

  setOffset(offset: f64): void {
    this.style.setOffset(-0.45, offset);
  }
}

let main_panel: PanelImpl;

export class PanelImpl {
  style_pool: Array<GlyphStyle> = [];
  message_queue: Array<QueuedMessage> = [];
  log_bottom: f64 = -0.45;
  last_message: f64 = 0.0;

  constructor(public panel: UiPanel) {
    main_panel = this;
  }

  selectAt(x: f64, y: f64): void {}

  handleMessage(message: string): void {
    let style: GlyphStyle;

    if (this.style_pool.length > 0) {
      style = this.style_pool.pop();
    } else {
      style = this.panel.createGlyphStyle();
    }

    let new_message = new QueuedMessage(message, style, this.log_bottom);
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
  }
}

export function handleMessage(message: string): void {
  main_panel.handleMessage(message);
}
