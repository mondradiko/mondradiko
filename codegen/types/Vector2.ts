// Mondradiko scripting API - AssemblyScript bindings
// https://mondradiko.github.io/

export default class Vector2 {
  /**
   * Instantiation
   */

  constructor(public x: f64, public y: f64) {}

  clone(): Vector2 { return new Vector2(this.x, this.y); }

  static readonly Zero: Vector2 = new Vector2(0.0, 0.0);

  /**
   * Vector properties
   */

  squaredLength(): f64 {
    return this.dot(this);
  }

  length(): f64 {
    return Math.sqrt(this.squaredLength());
  }

  /**
   * Vector operations
   */

  normalize(): void {
    this.scale(1.0 / this.length());
  }

  /**
   * Vector-to-vector arithmetic
   */

  add(other: Vector2): void {
    this.x += other.x;
    this.y += other.y;
  }

  sub(other: Vector2): void {
    this.x -= other.x;
    this.y -= other.y;
  }

  mul(other: Vector2): void {
    this.x *= other.x;
    this.y *= other.y;
  }

  div(other: Vector2): void {
    this.x /= other.x;
    this.y /= other.y;
  }

  /**
   * Vector-to-scalar arithmetic
   */

  scale(s: f64): void {
    this.x *= s;
    this.y *= s;
  }

  set(s: f64): void {
    this.x = s;
    this.y = s;
  }

  /**
   * Vector-to-vector math
   */

  dot(other: Vector2): f64 {
    return this.x * other.x + this.y * other.y;
  }

  squaredDistance(other: Vector2): f64 {
    const x = this.x - other.x;
    const y = this.y - other.y;

    return x * x + y * y;
  }

  distance(other: Vector2): f64 {
    return Math.sqrt(this.squaredDistance(other));
  }
}
