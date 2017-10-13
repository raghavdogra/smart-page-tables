#!/usr/bin/env python
# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")

# This is an example of tracing an event and printing custom fields.
# run in project examples directory with:
# sudo ./trace_fields.py"

from bcc import BPF

prog = """
#include <uapi/linux/ptrace.h>

int hello_free_pages(struct pt_regs *ctx, unsigned long addr, unsigned int order) {
  bpf_trace_printk("D\\t%d\\t%x\\n",(1<<order),addr);
  return 0;
}

int hook_retcode(struct pt_regs *regs, gfp_t gfp_mask, unsigned int order)
{
    unsigned long retval = 0;
    retval = regs_return_value(regs);
    bpf_trace_printk("A\\t%d\\t%x\\n",(1<<order),retval);
    return 0;
}
"""
b = BPF(text=prog)
b.attach_kprobe(event="free_pages", fn_name="hello_free_pages")
b.attach_kretprobe(event="__get_free_pages", fn_name="hook_retcode")
print "PID MESSAGE"
b.trace_print(fmt="{1}\t{5}")
