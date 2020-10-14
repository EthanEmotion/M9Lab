import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
/**
 * @class PiezoBuzzer
 * @extends Device
 */
export declare class PiezoBuzzer extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    /**
     * Play a tone on the Hub's in-built buzzer
     * @method PiezoBuzzer#playTone
     * @param {number} frequency
     * @param {number} time How long the tone should play for (in milliseconds).
     * @returns {Promise} Resolved upon successful completion of command (ie. once the tone has finished playing).
     */
    playTone(frequency: number, time: number): Promise<unknown>;
}
