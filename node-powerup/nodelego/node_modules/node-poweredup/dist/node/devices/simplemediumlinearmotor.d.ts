import { BasicMotor } from "./basicmotor";
import { IDeviceInterface } from "../interfaces";
/**
 * @class SimpleMediumLinearMotor
 * @extends Device
 */
export declare class SimpleMediumLinearMotor extends BasicMotor {
    constructor(hub: IDeviceInterface, portId: number);
}
