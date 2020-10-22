import { Peripheral } from "@abandonware/noble";
import { LPF2Hub } from "./lpf2hub";
import { IBLEAbstraction } from "../interfaces";
/**
 * The DuploTrainBase is emitted if the discovered device is a Duplo Train Base.
 * @class DuploTrainBase
 * @extends LPF2Hub
 * @extends BaseHub
 */
export declare class DuploTrainBase extends LPF2Hub {
    static IsDuploTrainBase(peripheral: Peripheral): boolean;
    constructor(device: IBLEAbstraction);
    connect(): Promise<void>;
}
export declare const PortMap: {
    [portName: string]: number;
};
