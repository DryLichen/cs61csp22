    jal a0, label2
label1:
    auipc t0, 321123
    j end
label2:
    jalr s1, a0, 0
end: