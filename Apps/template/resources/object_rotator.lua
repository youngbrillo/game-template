local node = {
    params = {
        can_rotate = true,
        rotate_speed = 45,
        rotate_axis = Vector3(0, 1, 0)
    }
}

function node:onInit() 
    print ("\t Rotator Script Initialized for: '".. self.entity:getName() .. "'")
end

function node:onFree() 
    print ("\t Rotator Script Fee'd for: '".. self.entity:getName() .. "'")

end

function node:onUpdate(dt) 
    if self.params.can_rotate then
        local t = self.entity:get(Transform3D)
        t:RotateAroundAxis(self.params.rotate_axis, self.params.rotate_speed * dt);
    end
end

return node;