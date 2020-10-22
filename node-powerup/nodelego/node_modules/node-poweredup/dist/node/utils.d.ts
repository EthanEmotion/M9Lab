/// <reference types="node" />
import { EventEmitter } from "events";
import { Device } from "./devices/device";
export declare const isWebBluetooth: boolean;
export declare const toHex: (value: number, length?: number) => string;
export declare const toBin: (value: number, length?: number) => string;
export declare const mapSpeed: (speed: number) => number;
export declare const decodeVersion: (version: number) => string;
export declare const decodeMACAddress: (address: Uint8Array) => string;
export declare const normalizeAngle: (angle: number) => number;
export declare const roundAngleToNearest90: (angle: number) => 0 | -180 | 90 | -90;
export declare const calculateRamp: (device: Device, fromPower: number, toPower: number, time: number) => EventEmitter;
