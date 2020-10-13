/// <reference types="node" />
import { BaseHub } from "./basehub";
/**
 * @class LPF2Hub
 * @extends BaseHub
 */
export declare class LPF2Hub extends BaseHub {
    private _messageBuffer;
    private _propertyRequestCallbacks;
    connect(): Promise<void>;
    /**
     * Shutdown the Hub.
     * @method LPF2Hub#shutdown
     * @returns {Promise} Resolved upon successful disconnect.
     */
    shutdown(): Promise<void>;
    /**
     * Set the name of the Hub.
     * @method LPF2Hub#setName
     * @param {string} name New name of the hub (14 characters or less, ASCII only).
     * @returns {Promise} Resolved upon successful issuance of command.
     */
    setName(name: string): Promise<void>;
    send(message: Buffer, uuid: string): Promise<void>;
    subscribe(portId: number, deviceType: number, mode: number): Promise<void>;
    unsubscribe(portId: number, mode: number): Promise<void>;
    /**
     * Combines two ports with into a single virtual port.
     *
     * Note: The devices attached to the ports must be of the same device type.
     * @method LPF2Hub#createVirtualPort
     * @param {string} firstPortName First port name
     * @param {string} secondPortName Second port name
     * @returns {Promise} Resolved upon successful issuance of command.
     */
    createVirtualPort(firstPortName: string, secondPortName: string): Promise<void>;
    protected _checkFirmware(version: string): void;
    private _parseMessage;
    private _requestHubPropertyValue;
    private _requestHubPropertyReports;
    private _parseHubPropertyResponse;
    private _parsePortMessage;
    private _sendPortInformationRequest;
    private _parsePortInformationResponse;
    private _sendModeInformationRequest;
    private _parseModeInformationResponse;
    private _parsePortAction;
    private _parseSensorMessage;
}
