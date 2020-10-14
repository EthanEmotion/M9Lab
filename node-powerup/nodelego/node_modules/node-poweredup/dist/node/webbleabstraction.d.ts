/// <reference types="node" />
import { EventEmitter } from "events";
import { IBLEAbstraction } from "./interfaces";
export declare class WebBLEDevice extends EventEmitter implements IBLEAbstraction {
    private _webBLEServer;
    private _uuid;
    private _name;
    private _listeners;
    private _characteristics;
    private _queue;
    private _mailbox;
    private _connected;
    private _connecting;
    constructor(device: any);
    get uuid(): string;
    get name(): string;
    get connecting(): boolean;
    get connected(): boolean;
    connect(): Promise<void>;
    disconnect(): Promise<void>;
    discoverCharacteristicsForService(uuid: string): Promise<void>;
    subscribeToCharacteristic(uuid: string, callback: (data: Buffer) => void): void;
    addToCharacteristicMailbox(uuid: string, data: Buffer): void;
    readFromCharacteristic(uuid: string, callback: (err: string | null, data: Buffer | null) => void): void;
    writeToCharacteristic(uuid: string, data: Buffer): Promise<any>;
    private _sanitizeUUID;
}
