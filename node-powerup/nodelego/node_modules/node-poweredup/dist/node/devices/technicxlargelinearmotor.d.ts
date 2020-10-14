import { AbsoluteMotor } from "./absolutemotor";
import { IDeviceInterface } from "../interfaces";
/**
 * @class TechnicXLargeLinearMotor
 * @extends AbsoluteMotor
 */
export declare class TechnicXLargeLinearMotor extends AbsoluteMotor {
    constructor(hub: IDeviceInterface, portId: number);
}
