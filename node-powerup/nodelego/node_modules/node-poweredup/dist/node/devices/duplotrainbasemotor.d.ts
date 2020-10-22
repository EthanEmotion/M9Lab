import { BasicMotor } from "./basicmotor";
import { IDeviceInterface } from "../interfaces";
/**
 * @class DuploTrainBaseMotor
 * @extends BasicMotor
 */
export declare class DuploTrainBaseMotor extends BasicMotor {
    constructor(hub: IDeviceInterface, portId: number);
}
