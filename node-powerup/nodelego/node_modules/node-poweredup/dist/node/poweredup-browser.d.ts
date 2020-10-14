/// <reference types="node" />
import { BaseHub } from "./hubs/basehub";
import { EventEmitter } from "events";
/**
 * @class PoweredUP
 * @extends EventEmitter
 */
export declare class PoweredUP extends EventEmitter {
    private _connectedHubs;
    constructor();
    /**
     * Begin scanning for Powered UP Hub devices.
     * @method PoweredUP#scan
     */
    scan(): Promise<boolean>;
    /**
     * Retrieve a list of Powered UP Hubs.
     * @method PoweredUP#getHubs
     * @returns {BaseHub[]}
     */
    getHubs(): BaseHub[];
    /**
     * Retrieve a Powered UP Hub by UUID.
     * @method PoweredUP#getHubByUUID
     * @param {string} uuid
     * @returns {BaseHub | null}
     */
    getHubByUUID(uuid: string): BaseHub;
    /**
     * Retrieve a Powered UP Hub by primary MAC address.
     * @method PoweredUP#getHubByPrimaryMACAddress
     * @param {string} address
     * @returns {BaseHub}
     */
    getHubByPrimaryMACAddress(address: string): BaseHub;
    /**
     * Retrieve a list of Powered UP Hub by name.
     * @method PoweredUP#getHubsByName
     * @param {string} name
     * @returns {BaseHub[]}
     */
    getHubsByName(name: string): BaseHub[];
    /**
     * Retrieve a list of Powered UP Hub by type.
     * @method PoweredUP#getHubsByType
     * @param {string} name
     * @returns {BaseHub[]}
     */
    getHubsByType(hubType: number): BaseHub[];
    private _determineLPF2HubType;
    private _discoveryEventHandler;
}
