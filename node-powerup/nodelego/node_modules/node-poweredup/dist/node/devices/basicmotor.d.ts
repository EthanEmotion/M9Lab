import { Device } from "./device";
import { IDeviceInterface } from "../interfaces";
import * as Consts from "../consts";
/**
 * @class BasicMotor
 * @extends Device
 */
export declare class BasicMotor extends Device {
    constructor(hub: IDeviceInterface, portId: number, modeMap: {
        [event: string]: number;
    }, type?: Consts.DeviceType);
    /**
     * Set the motor power.
     * @method BasicMotor#setPower
     * @param {number} power For forward, a value between 1 - 100 should be set. For reverse, a value between -1 to -100. Stop is 0.
     * @returns {Promise} Resolved upon successful issuance of the command.
     */
    setPower(power: number, interrupt?: boolean): Promise<void>;
    /**
     * Ramp the motor power.
     * @method BasicMotor#rampPower
     * @param {number} fromPower For forward, a value between 1 - 100 should be set. For reverse, a value between -1 to -100. Stop is 0.
     * @param {number} toPower For forward, a value between 1 - 100 should be set. For reverse, a value between -1 to -100. Stop is 0.
     * @param {number} time How long the ramp should last (in milliseconds).
     * @returns {Promise} Resolved upon successful completion of command.
     */
    rampPower(fromPower: number, toPower: number, time: number): Promise<unknown>;
    /**
     * Stop the motor.
     * @method BasicMotor#stop
     * @returns {Promise} Resolved upon successful issuance of the command.
     */
    stop(): Promise<void>;
    /**
     * Brake the motor.
     * @method BasicMotor#brake
     * @returns {Promise} Resolved upon successful issuance of the command.
     */
    brake(): Promise<void>;
}
