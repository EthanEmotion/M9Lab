import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
import * as Consts from "../consts";
/**
 * @class DuploTrainBaseSpeaker
 * @extends Device
 */
export declare class DuploTrainBaseSpeaker extends Device {
    constructor(hub: IDeviceInterface, portId: number);
    /**
     * Play a built-in train sound.
     * @method DuploTrainBaseSpeaker#playSound
     * @param {DuploTrainBaseSound} sound
     * @returns {Promise} Resolved upon successful issuance of the command.
     */
    playSound(sound: Consts.DuploTrainBaseSound): Promise<unknown>;
    /**
     * Play a built-in system tone.
     * @method DuploTrainBaseSpeaker#playTone
     * @param {number} tone
     * @returns {Promise} Resolved upon successful issuance of the command.
     */
    playTone(tone: number): void;
}
export declare enum Mode {
    SOUND = 1,
    TONE = 2
}
