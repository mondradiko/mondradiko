// Mondradiko scripting API - AssemblyScript bindings
// https://mondradiko.github.io/

export default class Vector3 {
  /**
   * Instantiation
   */

  constructor(public x: f64, public y: f64, public z: f64) {}

  clone(): Vector3 { return new Vector3(this.x, this.y, this.z); }

  static readonly Zero: Vector3 = new Vector3(0.0, 0.0, 0.0);
  static readonly Forward: Vector3 = new Vector3(0.0, 0.0, 1.0);
  static readonly Up: Vector3 = new Vector3(0.0, 1.0, 0.0);

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

  add(other: Vector3): void {
    this.x += other.x;
    this.y += other.y;
    this.z += other.z;
  }

  sub(other: Vector3): void {
    this.x -= other.x;
    this.y -= other.y;
    this.z -= other.z;
  }

  mul(other: Vector3): void {
    this.x *= other.x;
    this.y *= other.y;
    this.z *= other.z;
  }

  div(other: Vector3): void {
    this.x /= other.x;
    this.y /= other.y;
    this.z /= other.z;
  }

  /**
   * Vector-to-scalar arithmetic
   */

  scale(s: f64): void {
    this.x *= s;
    this.y *= s;
    this.z *= s;
  }

  set(s: f64): void {
    this.x = s;
    this.y = s;
    this.z = s;
  }

  /**
   * Vector-to-vector math
   */

  static cross(lhs: Vector3, rhs: Vector3, dest: Vector3): Vector3 {
    const x1 = lhs.x;
    const y1 = lhs.y;
    const z1 = lhs.z;

    const x2 = rhs.x;
    const y2 = rhs.y;
    const z2 = rhs.z;

    dest.x = y1 * z2 - z1 * y2;
    dest.y = z1 * x2 - x1 * z2;
    dest.z = x1 * y2 - y1 * x2;

    return dest;
  }

  dot(other: Vector3): f64 {
    return this.x * other.x + this.y * other.y + this.z * other.z;
  }

  squaredDistance(other: Vector3): f64 {
    const x = this.x - other.x;
    const y = this.y - other.y;
    const z = this.z - other.z;

    return x * x + y * y + z * z;
  }

  distance(other: Vector3): f64 {
    return Math.sqrt(this.squaredDistance(other));
  }
}
