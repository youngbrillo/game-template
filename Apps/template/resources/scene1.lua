function math.randomFloat(min, max)
    return math.random() * (max - min) + min
end

local scene = {

}

function scene:onInit()
    print "\t Initializing Scene 1"
    -- self:create_random_boxes();
    self:create_random_spheres();
end

function scene:onFree()
    print "\t Freeing Scene 1"

end

function scene:create_random_boxes()
    local t = Transform3D()
    local rgb = Color(0XFFFFFFFF);
    local axis = Vector3(0, 1, 0);
    local angle = 0.;
    for i = 1, 40, 1 do
        t.position:set(
            math.randomFloat(-25, 25),
            math.randomFloat(5, 25),
            math.randomFloat(-25, 25)
        )

        axis:set(
            math.random(),
            math.random(),
            math.random()
        );
        axis:normalize();
        angle = math.randomFloat(-365, 365);
        t:RotateAroundAxis(axis, angle);
        rgb.a = math.random(0, 255)
        rgb.g = math.random(0, 255)
        rgb.b = math.random(0, 255)
        
        CreateBox(t, rgb);
    end
end

function scene:create_random_spheres()
    local v = Vector3(0, 0, 0);
    local rgb = Color(0XFFFFFFFF);

    local p = CreateEntity("spheres")
    p:disableSerialization();
    for i = 1, 40, 1 do
        v:set(
            math.randomFloat(-25, 25),
            math.randomFloat(5, 25),
            math.randomFloat(-25, 25)
        )
        rgb.a = math.random(0, 255)
        rgb.g = math.random(0, 255)
        rgb.b = math.random(0, 255)
        
        local child = CreateSphere(v, rgb);
        child:addParent(p);
    end
end

return scene;
