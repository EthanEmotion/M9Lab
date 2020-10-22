import { BasicMotor } from "./basicmotor";
import { IDeviceInterface } from "../interfaces";
/**
 * @class TrainMotor
 * @extends BasicMotor
 */
export declare class TrainMotor extends BasicMotor {
    constructor(hub: IDeviceInterface, portId: number);
}
