/// <reference types="node" />
import { EventEmitter } from "events";
import { IBLEAbstraction } from "../interfaces";
import { Device } from "../devices/device";
import * as Consts from "../consts";
/**
 * @class BaseHub
 * @extends EventEmitter
 */
export declare class BaseHub extends EventEmitter {
    protected _attachedDevices: {
        [portId: number]: Device;
    };
    protected _name: string;
    protected _firmwareVersion: string;
    protected _hardwareVersion: string;
    protected _primaryMACAddress: string;
    protected _batteryLevel: number;
    protected _rssi: number;
    protected _portMap: {
        [portName: string]: number;
    };
    protected _virtualPorts: number[];
    protected _bleDevice: IBLEAbstraction;
    private _type;
    private _attachCallbacks;
    constructor(bleDevice: IBLEAbstraction, portMap?: {
        [portName: string]: number;
    }, type?: Consts.HubType);
    /**
     * @readonly
     * @property {string} name Name of the hub
     */
    get name(): string;
    /**
     * @readonly
     * @property {string} type Hub type
     */
    get type(): Consts.HubType;
    /**
     * @readonly
     * @property {string[]} ports Array of port names
     */
    get ports(): string[];
    /**
     * @readonly
     * @property {string} firmwareVersion Firmware version of the hub
     */
    get firmwareVersion(): string;
    /**
     * @readonly
     * @property {string} hardwareVersion Hardware version of the hub
     */
    get hardwareVersion(): string;
    /**
     * @readonly
     * @property {string} primaryMACAddress Primary MAC address of the hub
     */
    get primaryMACAddress(): string;
    /**
     * @readonly
     * @property {string} uuid UUID of the hub
     */
    get uuid(): string;
    /**
     * @readonly
     * @property {number} batteryLevel Battery level of the hub (Percentage between 0-100)
     */
    get batteryLevel(): number;
    /**
     * @readonly
     * @property {number} rssi Signal strength of the hub
     */
    get rssi(): number;
    /**
     * Connect to the Hub.
     * @method Hub#connect
     * @returns {Promise} Resolved upon successful connect.
     */
    connect(): Promise<void>;
    /**
     * Disconnect the Hub.
     * @method Hub#disconnect
     * @returns {Promise} Resolved upon successful disconnect.
     */
    disconnect(): Promise<void>;
    /**
     * Retrieves the device attached to a given port.
     * @method Hub#getDeviceAtPort
     * @param {string} portName The name of the port to retrieve the device from.
     * @returns {Device | undefined} The device attached to the port.
     */
    getDeviceAtPort(portName: string): Device | undefined;
    /**
     * Retrieves the device attached to a given port, waiting until one is attached if there isn't one.
     *
     * Note: If a device is never attached, the returned promise may never resolve.
     * @method Hub#waitForDeviceAtPort
     * @param {string} portName The name of the port to retrieve the device from.
     * @returns {Promise} Resolved once a device is attached, or resolved immediately if a device is already attached.
     */
    waitForDeviceAtPort(portName: string): Promise<unknown>;
    /**
     * Retrieves all attached devices.
     * @method Hub#getDevices
     * @returns {Device[]} Array of all attached devices.
     */
    getDevices(): Device[];
    /**
     * Retrieves an array of devices of the specified type.
     * @method Hub#getDevicesByType
     * @param {number} deviceType The device type to lookup.
     * @returns {Device[]} Array of all devices of the specified type.
     */
    getDevicesByType(deviceType: number): Device[];
    /**
     * Retrieves the first device attached of the specified type, waiting until one is attached if there isn't one.
     *
     * Note: If a device is never attached, the returned promise may never resolve.
     * @method Hub#waitForDeviceByType
     * @param {number} deviceType The device type to lookup.
     * @returns {Promise} Resolved once a device is attached, or resolved immediately if a device is already attached.
     */
    waitForDeviceByType(deviceType: number): Promise<unknown>;
    getPortNameForPortId(portId: number): string | undefined;
    isPortVirtual(portId: number): boolean;
    /**
     * Sleep a given amount of time.
     *
     * Note: This is a helper method to make it easier to add delays into a chain of commands.
     * @method Hub#sleep
     * @param {number} delay How long to sleep (in milliseconds).
     * @returns {Promise} Resolved after the delay is finished.
     */
    sleep(delay: number): Promise<unknown>;
    /**
     * Wait until a given list of concurrently running commands are complete.
     *
     * Note: This is a helper method to make it easier to wait for concurrent commands to complete.
     * @method Hub#wait
     * @param {Array<Promise<any>>} commands Array of executing commands.
     * @returns {Promise} Resolved after the commands are finished.
     */
    wait(commands: Promise<any>[]): Promise<any[]>;
    send(message: Buffer, uuid: string): Promise<void>;
    subscribe(portId: number, deviceType: number, mode: number): void;
    unsubscribe(portId: number, deviceType: number, mode: number): void;
    manuallyAttachDevice(deviceType: number, portId: number): Device;
    protected _attachDevice(device: Device): void;
    protected _detachDevice(device: Device): void;
    protected _createDevice(deviceType: number, portId: number): Device;
    protected _getDeviceByPortId(portId: number): Device;
}
