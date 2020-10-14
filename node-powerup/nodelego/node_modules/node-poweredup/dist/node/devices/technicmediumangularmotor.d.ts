import { AbsoluteMotor } from "./absolutemotor";
import { IDeviceInterface } from "../interfaces";
import * as Consts from "../consts";
/**
 * @class TechnicMediumAngularMotor
 * @extends AbsoluteMotor
 */
export declare class TechnicMediumAngularMotor extends AbsoluteMotor {
    constructor(hub: IDeviceInterface, portId: number, modeMap?: {
        [event: string]: number;
    }, type?: Consts.DeviceType);
}
