
export interface GDImg {
  destroy(): void;
  setPixel(x: number, y: number, color: number): void;
  line(x1: number, y1: number, x2: number, y2: number, color: number): void;

  // Feelin' lazy
  [key: string]: (...args: any[]) => void;
}

export enum speed {
  normal,
  quick,
  quickest
}

export const colors: {
  white: number,
  gray: number,
  darkgray: number,
  black: number
}
export const width: number;
export const height: number;

export function getImageBuffer(orientation?: 'portrait' | 'landscape'): Promise<GDImg>;
export function init(sp?: speed): Promise<void>;
export function initGray(sp?: speed): Promise<void>;
export function clear(): Promise<void>;
export function displayImageBuffer(img: GDImg): Promise<void>;
export function displayGrayImageBuffer(img: GDImg): Promise<void>;
export function sleep(): Promise<void>;
